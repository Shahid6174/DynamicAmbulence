'use client';

import { useRouter, useSearchParams } from 'next/navigation';
import { useEffect, useState } from 'react';

export default function AddPatientPage() {
  const router = useRouter();
  const params = useSearchParams();
  const [loading, setLoading] = useState(false);
  const [formData, setFormData] = useState({
    _id: '',
    name: '',
    age: '',
    bloodGroup: '',
    patientId: '',
    vaccinesDone: '',
    areaOfTreatment: '',
    insurance: '',
    phoneNumber: '',
    hospitalAssigned: '',
    optimalCost: '',
  });

  useEffect(() => {
    // Load from query parameters if they exist
    const updatedData = {};
    for (const [key, value] of params.entries()) {
      updatedData[key] = value;
    }
    setFormData((prev) => ({ ...prev, ...updatedData }));
  }, [params]);

  const handleChange = (e) => {
    const { id, value } = e.target;
    setFormData((prev) => ({ ...prev, [id]: value }));
  };

  const handleSubmit = async (e) => {
    e.preventDefault();
    setLoading(true);

    const payload = {
      name: formData.name,
      age: parseInt(formData.age),
      bloodGroup: formData.bloodGroup,
      patientId: formData.patientId,
      vaccinesDone: formData.vaccinesDone,
      areaOfTreatment: formData.areaOfTreatment,
      insurance: formData.insurance,
      phoneNumber: formData.phoneNumber,
      hospitalAssigned: formData.hospitalAssigned,
      optimalCost: parseFloat(formData.optimalCost),
    };

    const method = formData._id ? 'PUT' : 'POST';
    const body = formData._id
      ? { filter: { _id: formData._id }, update: payload }
      : { data: payload };

    try {
      const res = await fetch(`/api/mongo?collection=patients`, {
        method,
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify(body),
      });

      const result = await res.json();
      if (res.ok) {
        alert(`✅ Patient ${method === 'POST' ? 'added' : 'updated'} successfully!`);
     
      } else {
        alert(`❌ Error: ${result?.error || 'Unknown error'}`);
      }
    } catch (err) {
      console.error(err);
      alert('❌ Network error');
    } finally {
      setLoading(false);
    }
  };

  return (
    <div className="container py-5">
      <div className="row justify-content-center">
        <div className="col-lg-8">
          <div className="card shadow rounded-4">
            <div className="card-body p-4">
              <h4 className="fw-bold mb-4 text-primary">
                {formData._id ? 'Edit Patient' : 'Add New Patient'}
              </h4>
              <form onSubmit={handleSubmit}>
                <div className="row g-3">
                  {[
                    ['name', 'Full Name'],
                    ['age', 'Age', 'number'],
                    ['patientId', 'Patient ID'],
                    ['areaOfTreatment', 'Area of Treatment'],
                    ['phoneNumber', 'Phone Number'],
                    ['hospitalAssigned', 'Hospital Assigned'],
                    ['optimalCost', 'Optimal Cost (INR)', 'number'],
                  ].map(([id, label, type = 'text']) => (
                    <div className="col-md-6" key={id}>
                      <div className="form-floating">
                        <input
                          type={type}
                          id={id}
                          className="form-control"
                          value={formData[id] || ''}
                          onChange={handleChange}
                          required
                        />
                        <label htmlFor={id}>{label} *</label>
                      </div>
                    </div>
                  ))}

                  <div className="col-md-3">
                    <div className="form-floating">
                      <select
                        id="bloodGroup"
                        className="form-select"
                        value={formData.bloodGroup}
                        onChange={handleChange}
                        required
                      >
                        <option value="">Select</option>
                        {['A+', 'A-', 'B+', 'B-', 'AB+', 'AB-', 'O+', 'O-'].map((bg) => (
                          <option key={bg} value={bg}>{bg}</option>
                        ))}
                      </select>
                      <label htmlFor="bloodGroup">Blood Group *</label>
                    </div>
                  </div>

                  <div className="col-md-3">
                    <div className="form-floating">
                      <select
                        id="vaccinesDone"
                        className="form-select"
                        value={formData.vaccinesDone}
                        onChange={handleChange}
                        required
                      >
                        <option value="">Select</option>
                        <option value="y">Yes</option>
                        <option value="n">No</option>
                      </select>
                      <label htmlFor="vaccinesDone">Vaccines Done *</label>
                    </div>
                  </div>

                  <div className="col-md-6">
                    <div className="form-floating">
                      <select
                        id="insurance"
                        className="form-select"
                        value={formData.insurance}
                        onChange={handleChange}
                        required
                      >
                        <option value="">Select</option>
                        <option value="yes">Yes</option>
                        <option value="no">No</option>
                      </select>
                      <label htmlFor="insurance">Insurance *</label>
                    </div>
                  </div>
                </div>

                <div className="d-flex gap-3 mt-4">
                  <button type="submit" className="btn btn-primary px-4 shadow-sm" disabled={loading}>
                    {loading ? 'Saving...' : formData._id ? 'Update Patient' : 'Save Patient'}
                  </button>
                  <button
                    type="button"
                    className="btn btn-outline-secondary px-4"
                    onClick={() => router.push('/AdminDashboard')}
                  >
                    Cancel
                  </button>
                </div>
              </form>
            </div>
          </div>
        </div>
      </div>
    </div>
  );
}
